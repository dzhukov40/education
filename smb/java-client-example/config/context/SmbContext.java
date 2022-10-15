package ****.config.context;

import lombok.Value;
import ****.integration.client.smb.SmbClient;

import java.net.URI;

@Value(staticConstructor = "of")
public class SmbContext {
    SmbClient.PasswordAuthentication depAuth;
    SmbClient.PasswordAuthentication svpcAuth;

    URI depIncomeFolderPath;
    URI depOutcomeFolderPath;

    URI svpcIncomeFolderPath;
    URI svpcOutcomeFolderPath;
}
