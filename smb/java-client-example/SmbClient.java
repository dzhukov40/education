package ****.integration.client.smb;

import lombok.Value;

import java.net.URI;
import java.time.LocalDateTime;
import java.util.List;
import java.util.Map;

public interface SmbClient {

    boolean isFileExist(URI uri, PasswordAuthentication auth, String path);

    boolean isFolderExist(URI uri, PasswordAuthentication auth, String path);

    void mkdir(URI uri, PasswordAuthentication auth, String path);

    void mkdirWithParentDirectories(URI uri, PasswordAuthentication auth, String path);

    FileInformation getFileInformation(URI uri, PasswordAuthentication auth, String path);

    List<FileInformation> listFiles(URI uri, PasswordAuthentication auth, String path);

    void removeFile(URI uri, PasswordAuthentication auth, String path);

    byte[] readFile(URI uri, PasswordAuthentication auth, String path);

    void writeFile(URI uri, PasswordAuthentication auth, String path, byte[] content);

    void writeFiles(URI uri, PasswordAuthentication auth, String path, Map<String, byte[]> files);

    @Value(staticConstructor = "of")
    class PasswordAuthentication {
        String domain;
        String username;
        String password;
    }

    @Value(staticConstructor = "of")
    class FileInformation {
        LocalDateTime creationTime;
        LocalDateTime lastAccessTime;
        LocalDateTime lastWriteTimeTime;
        LocalDateTime changeTime;
        String fullFileName;
        String shortFileName;
        boolean isDirectory;
    }

}
