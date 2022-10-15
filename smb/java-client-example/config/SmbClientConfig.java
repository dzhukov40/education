package ****.config;

import com.hierynomus.smbj.SMBClient;
import com.hierynomus.smbj.SmbConfig;
import lombok.RequiredArgsConstructor;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import ****.common.SmbPathBuilder;
import ****.config.context.SmbContext;
import ****.config.properties.SmbAddressProperties;
import ****.config.properties.SmbAuthProperties;
import ****.config.properties.SmbDepProperties;
import ****.config.properties.SmbSvpcProperties;
import ****.integration.client.smb.SmbClient;

import java.net.URI;

@Configuration
@RequiredArgsConstructor
public class SmbClientConfig {

    private final SmbSvpcProperties smbSvpcProperties;
    private final SmbDepProperties smbDepProperties;

    //TODO: add params to smbConfig from application-context.yaml
    @Bean
    public SMBClient smbClient() {
        SmbConfig cfg = SmbConfig.builder()
                .build();

        return new SMBClient(cfg);
    }

    @Bean
    public SmbContext smbContext() {
        var depAuth = generatePasswordAuthentication(smbDepProperties);
        var svpcAuth = generatePasswordAuthentication(smbSvpcProperties);

        var depIncomeFolderPath = generateFolderPath(smbDepProperties, smbDepProperties.getIncomeFolderPath());
        var depOutcomeFolderPath = generateFolderPath(smbDepProperties, smbDepProperties.getOutcomeFolderPath());

        var svpcIncomeFolderPath = generateFolderPath(smbSvpcProperties, smbSvpcProperties.getIncomeFolderPath());
        var svpcOutcomeFolderPath = generateFolderPath(smbSvpcProperties, smbSvpcProperties.getOutcomeFolderPath());

        return SmbContext.of(
                depAuth,
                svpcAuth,
                depIncomeFolderPath,
                depOutcomeFolderPath,
                svpcIncomeFolderPath,
                svpcOutcomeFolderPath
        );
    }

    private static URI generateFolderPath(SmbAddressProperties addressProperties, String folderPath) {
        return SmbPathBuilder.buildUri(
                addressProperties.getHost(),
                addressProperties.getPort(),
                folderPath + "/");
    }

    private static SmbClient.PasswordAuthentication generatePasswordAuthentication(SmbAuthProperties authProperties) {
        return SmbClient.PasswordAuthentication
                .of(authProperties.getDomain(), authProperties.getUsername(), authProperties.getPassword());
    }

}
