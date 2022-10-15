package ****.config.properties;

import lombok.Data;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;


@Data
@Configuration
@ConfigurationProperties(prefix = "smb.svpc")
public class SmbSvpcProperties implements SmbAuthProperties, SmbAddressProperties {

    /**
     * Домен
     */
    private String domain;

    /**
     * Хост
     */
    private String host;

    /**
     * Порт
     */
    private Integer port = 445;

    /**
     * Имя пользователя
     */
    private String username;

    /**
     * Пароль
     */
    private String password;

    /**
     * Путь до папки ВХОДЯЩИЕ
     */
    private String incomeFolderPath;

    /**
     * Путь до папки ИСХОДЯЩИЕ
     */
    private String outcomeFolderPath;

    /**
     * Интервал проверки папки
     */
    private int checkInterval;

}
