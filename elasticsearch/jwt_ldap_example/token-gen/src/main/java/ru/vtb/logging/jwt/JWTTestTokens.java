package ru.vtb.logging.jwt;

import io.jsonwebtoken.Jwts;
import io.jsonwebtoken.io.Decoders;
import io.jsonwebtoken.io.Encoders;
import io.jsonwebtoken.security.Keys;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;

import java.security.Key;
import java.util.Date;
import java.util.List;

import static java.lang.System.currentTimeMillis;

/**
 RUN [ java -jar target/jwt-test-tokens-1.0-SNAPSHOT-jar-with-dependencies.jar ]

 kibanaAdmin: eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJraWJhbmFBZG1pbiIsImlzcyI6Imh0dHBzOi8vbG9jYWxob3N0IiwiZXhwIjoxNjkwMDM4MjQ0fQ.pKOBygKY7IRIabhqBY6WCUzVul4TDSfSOVGMB_6b_x0
 psantos: eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJwc2FudG9zIiwiaXNzIjoiaHR0cHM6Ly9sb2NhbGhvc3QiLCJleHAiOjE2OTAwMzgyNDR9.MKWq1-jMLT91u8xTvJCI3bZmf4tK9z_gt3xytvUeROg
 jroe: eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJqcm9lIiwiaXNzIjoiaHR0cHM6Ly9sb2NhbGhvc3QiLCJleHAiOjE2OTAwMzgyNDR9.Nun9Crbme0r3r3Os2eYV-zjhyJf-25Bi2QBDGgNbEq0
 Shared secret: fyqouR0whwaQ/eBh/eGLa1SmoQ3+vpuFzYxav0khrog=
 */
@Slf4j
public class JWTTestTokens {

    private static final String DEFAULT_ISSUER = "https://localhost";
    private static final String DEFAULT_SECRET = "fyqouR0whwaQ/eBh/eGLa1SmoQ3+vpuFzYxav0khrog=";

    public static void main(String[] args) {
        Key key = Keys.hmacShaKeyFor(Decoders.BASE64.decode(getSecret(args)));
        var jwtSubjects = List.of("kibanaAdmin", "psantos","jroe");

        for (var jwtSubject : jwtSubjects) {
            var jwt = buildJwt(jwtSubject, getIssuer(args), key);
            log.info("{}: {}", jwtSubject, jwt);
        }

        log.info("Shared secret: {}", Encoders.BASE64.encode(key.getEncoded()));
    }

    private static String getIssuer(String[] args) {
        return args.length < 1 || StringUtils.isEmpty(args[0]) ? DEFAULT_ISSUER : args[0];
    }

    private static String getSecret(String[] args) {
        return args.length < 2 || StringUtils.isEmpty(args[1]) ? DEFAULT_SECRET : args[0];
    }

    private static String buildJwt(String sub, String issuer, Key key) {
        return Jwts.builder()
                .setIssuer(issuer)
                .setSubject(sub)
                .setExpiration(new Date(currentTimeMillis() + 90000000000L))
                .signWith(key).compact();
    }

}
