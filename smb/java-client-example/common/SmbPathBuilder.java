package ****.common;

import lombok.SneakyThrows;
import lombok.experimental.UtilityClass;
import org.apache.http.client.utils.URIBuilder;

import java.net.URI;

import static java.util.Optional.ofNullable;

@UtilityClass
public class SmbPathBuilder {

    private final String DEFAULT_HOST = "localhost";
    private final Integer DEFAULT_PORT = 455;

    public static String build(String host, Integer port, String path) {
       return "smb://" +
               ofNullable(host).orElse(DEFAULT_HOST) +
               ":" +
               ofNullable(port).orElse(DEFAULT_PORT) +
               "/" +
               ofNullable(path).orElse("");
    }

    public static URI buildUri(String host, Integer port, String path) {
        return URI.create(build(host, port, path));
    }

    @SneakyThrows
    public static URI changeUriPath(URI uri, String path) {
        return new URIBuilder(uri)
                .setPath(path)
                .build();
    }

}
