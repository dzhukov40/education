package ****.integration.client.smb.impl;

import com.hierynomus.msdtyp.AccessMask;
import com.hierynomus.msfscc.fileinformation.FileAllInformation;
import com.hierynomus.msfscc.fileinformation.FileIdBothDirectoryInformation;
import com.hierynomus.mssmb2.SMB2ShareAccess;
import com.hierynomus.smbj.SMBClient;
import com.hierynomus.smbj.auth.AuthenticationContext;
import com.hierynomus.smbj.connection.Connection;
import com.hierynomus.smbj.session.Session;
import com.hierynomus.smbj.share.DiskShare;
import com.hierynomus.smbj.share.File;
import lombok.RequiredArgsConstructor;
import lombok.SneakyThrows;
import org.apache.commons.collections4.functors.ClosureTransformer;
import org.apache.commons.io.IOUtils;
import org.apache.commons.lang3.StringUtils;
import org.apache.logging.log4j.util.Strings;
import org.springframework.stereotype.Service;
import ****.integration.client.smb.SmbClient;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URI;
import java.util.EnumSet;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.stream.Collectors;

import static com.hierynomus.mssmb2.SMB2CreateDisposition.FILE_OPEN;
import static com.hierynomus.mssmb2.SMB2CreateDisposition.FILE_OVERWRITE_IF;
import static ****.common.FormatUtils.convertToLocalDateTimeViaInstant;

@Service
@RequiredArgsConstructor
public class SMBJSmbClientImpl implements SmbClient {

    private static final String CURRENT_DIRECTORY_REFERENCE = ".";
    private static final String PARENT_DIRECTORY_REFERENCE = "..";
    private static final String WINDOWS_PATH_DELIMITER = "\\";
    private static final String LINUX_PATH_DELIMITER = "/";

    private final SMBClient smbClient;

    /**
     * Проверяет существование файла 
     * 
     * @param uri Адресс сетевой папки. Пример: smb://localhost:1445/dep-in/
     * @param auth Параметры аутентификации
     * @param path полный путь до файла в сетевой папке
     *                 
     * @return TRUE файл есть, FALSE файла нет.
     */
    @Override
    public boolean isFileExist(URI uri, PasswordAuthentication auth, String path) {
        var pathStartingAtShare = generatePathStartingAtShare(uri, path);
        return functionWrapper(uri, auth, (DiskShare share) -> isFileExist(share, pathStartingAtShare));
    }

    @Override
    public boolean isFolderExist(URI uri, PasswordAuthentication auth, String path) {
        var pathStartingAtShare = generatePathStartingAtShare(uri, path);
        return functionWrapper(uri, auth, (DiskShare share) -> isFolderExist(share, pathStartingAtShare));
    }

    @Override
    public void mkdir(URI uri, PasswordAuthentication auth, String path) {
        var pathStartingAtShare = generatePathStartingAtShare(uri, path);
        consumerWrapper(uri, auth, (DiskShare share) -> mkdir(share, pathStartingAtShare));
    }

    @Override
    public void mkdirWithParentDirectories(URI uri, PasswordAuthentication auth, String path) {
        var pathStartingAtShare = generatePathStartingAtShare(uri, path);
        consumerWrapper(uri, auth, (DiskShare share) -> mkdirWithParentDirectories(share, pathStartingAtShare));
    }

    @Override
    public FileInformation getFileInformation(URI uri, PasswordAuthentication auth, String path) {
        var pathStartingAtShare = generatePathStartingAtShare(uri, path);
        return functionWrapper(uri, auth, (DiskShare share) -> getFileInformation(share, pathStartingAtShare));
    }

    @Override
    public List<FileInformation> listFiles(URI uri, PasswordAuthentication auth, String path) {
        var pathStartingAtShare = generatePathStartingAtShare(uri, path);
        return functionWrapper(uri, auth, (DiskShare share) -> listFiles(share, pathStartingAtShare));
    }

    @Override
    public void removeFile(URI uri, PasswordAuthentication auth, String path) {
        var pathStartingAtShare = generatePathStartingAtShare(uri, path);
        consumerWrapper(uri, auth, (DiskShare share) -> removeFile(share, pathStartingAtShare));
    }

    @Override
    public byte[] readFile(URI uri, PasswordAuthentication auth, String path) {
        var pathStartingAtShare = generatePathStartingAtShare(uri, path);
        return functionWrapper(uri, auth, (DiskShare share) -> readFile(share, pathStartingAtShare));
    }

    @Override
    public void writeFile(URI uri, PasswordAuthentication auth, String path, byte[] content) {
        var pathStartingAtShare = generatePathStartingAtShare(uri, path);
        consumerWrapper(uri, auth, (DiskShare share) -> writeFile(share, pathStartingAtShare, content));
    }

    @Override
    public void writeFiles(URI uri, PasswordAuthentication auth, String path, Map<String, byte[]> files) {
        var pathStartingAtShare = generatePathStartingAtShare(uri, path);
        consumerWrapper(uri, auth, (DiskShare share) -> writeFiles(share, pathStartingAtShare, files));
    }

    @SneakyThrows
    private static byte[] readFile(DiskShare share, String path) {
        File file = share.openFile(path,
                EnumSet.of(AccessMask.GENERIC_READ),
                null,
                SMB2ShareAccess.ALL,
                FILE_OPEN,
                null);

        try (InputStream ifs = file.getInputStream()) {
            return IOUtils.toByteArray(ifs);
        }
    }

    @SneakyThrows
    private static void writeFile(DiskShare share, String path, byte[] content) {
        File file = share.openFile(path,
                EnumSet.of(AccessMask.GENERIC_WRITE),
                null,
                SMB2ShareAccess.ALL,
                FILE_OVERWRITE_IF,
                null);

        try (OutputStream ofs = file.getOutputStream()) {
            ofs.write(content);
        }
    }

    private static boolean isFileExist(DiskShare share, String path) {
        return share.fileExists(path);
    }

    private static boolean isFolderExist(DiskShare share, String path) {
        return share.folderExists(path);
    }

    private static void mkdir(DiskShare share, String path) {
        share.mkdir(path);
    }

    private static void mkdirWithParentDirectories(DiskShare share, String path) {
        String creationPath = null;
        for (String folderName : path.split(LINUX_PATH_DELIMITER)) {
            creationPath = Strings.isEmpty(creationPath)
                    ? folderName
                    : String.join(LINUX_PATH_DELIMITER, creationPath, folderName);

            if ( ! share.folderExists(creationPath)) {
                share.mkdir(creationPath);
            }
        }
    }

    private static void removeFile(DiskShare share, String path) {
        share.rm(path);
    }

    private static List<FileInformation> listFiles(DiskShare share, String path) {
        List<String> files = share.list(path).stream()
                .map(FileIdBothDirectoryInformation::getFileName)
                .filter(Predicate.not(CURRENT_DIRECTORY_REFERENCE::equals))
                .filter(Predicate.not(PARENT_DIRECTORY_REFERENCE::equals))
                .map(value -> String.join("", path, value))
                .collect(Collectors.toList());

        return files.stream()
                .map(fileName -> getFileInformation(share, fileName))
                .collect(Collectors.toList());
    }

    private static FileInformation getFileInformation(DiskShare share, String path) {
        return generateFileInformation(share.getFileInformation(path), path);
    }

    private static FileInformation generateFileInformation(FileAllInformation targetFileInformation, String path) {
        var basicInformationOfTargetFile = targetFileInformation.getBasicInformation();
        var standardInformationOfTargetFile = targetFileInformation.getStandardInformation();

        return FileInformation.of(
                convertToLocalDateTimeViaInstant(basicInformationOfTargetFile.getCreationTime().toDate()),
                convertToLocalDateTimeViaInstant(basicInformationOfTargetFile.getLastAccessTime().toDate()),
                convertToLocalDateTimeViaInstant(basicInformationOfTargetFile.getLastWriteTime().toDate()),
                convertToLocalDateTimeViaInstant(basicInformationOfTargetFile.getChangeTime().toDate()),
                StringUtils.strip(path, LINUX_PATH_DELIMITER),
                path.substring(path.lastIndexOf(LINUX_PATH_DELIMITER) + 1),
                standardInformationOfTargetFile.isDirectory()
        );
    }

    private static void writeFiles(DiskShare share, String path, Map<String, byte[]> files) {
        for (Map.Entry<String, byte[]> file : files.entrySet()) {
            var fullPath = Strings.isEmpty(path)
                    ? file.getKey()
                    : String.join(LINUX_PATH_DELIMITER, path, file.getKey());

            writeFile(share, fullPath, file.getValue());
        }
    }

    private static String generatePathStartingAtShare(URI uri, String path) {
        var shareName = getShareName(uri);
        var sharePathWithoutShareName = uri.getPath().replace(shareName, "");
        var pathStartingAtShare = StringUtils.strip(sharePathWithoutShareName, LINUX_PATH_DELIMITER);

        return LINUX_PATH_DELIMITER.equals(pathStartingAtShare) || Strings.isEmpty(pathStartingAtShare)
                ? path
                : String.join(LINUX_PATH_DELIMITER, pathStartingAtShare, path);
    }

    private static String getShareName(URI uri) {
        var shareNameWithPath = StringUtils.strip(uri.getPath(), LINUX_PATH_DELIMITER);
        return shareNameWithPath.contains(LINUX_PATH_DELIMITER)
                ? shareNameWithPath.substring(0, shareNameWithPath.indexOf(LINUX_PATH_DELIMITER))
                : shareNameWithPath;
    }

    private AuthenticationContext generateAuth(PasswordAuthentication auth) {
        return new AuthenticationContext(auth.getUsername(), auth.getPassword().toCharArray(), auth.getDomain());
    }

    private void consumerWrapper(URI uri, PasswordAuthentication auth, Consumer<DiskShare> action) {
        functionWrapper(uri, auth, ClosureTransformer.closureTransformer(action::accept)::transform);
    }

    private <R> R functionWrapper(URI uri, PasswordAuthentication auth, Function<DiskShare, R> action) {
        try (Connection connection = smbClient.connect(uri.getHost(), uri.getPort())) {
            AuthenticationContext ac = generateAuth(auth);
            Session session = connection.authenticate(ac);

            try (DiskShare share = (DiskShare) session.connectShare(getShareName(uri))) {
                return action.apply(share);
            }
        } catch (IOException e) {
            throw new IllegalStateException(e);
        }
    }

}
