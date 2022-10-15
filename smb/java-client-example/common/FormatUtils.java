package ****.common;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.Date;

public class FormatUtils {

    public static final String DDMMYY = "ddMMyy";

    public static final String DATE_TIME_FORMAT = "yyyy-MM-dd'T'hh:mm:ss.SSSZ";

    public static final String DD_MM_YYYY = "dd.MM.yyyy";

    public static final String HH_MM_SS_DD_MM_YYYY = "HH:mm:ss dd.MM.yyyy";

    public static final DateTimeFormatter HH_MM_SS_DD_MM_YYYY_FORMATTER = DateTimeFormatter.ofPattern(HH_MM_SS_DD_MM_YYYY);

    public static Date stringDDMMYYToDate(String date) throws ParseException {
        return new SimpleDateFormat(DDMMYY).parse(date);
    }

    public static String dateToDayMonthYearTimeString(Date date) {
        return new SimpleDateFormat(DATE_TIME_FORMAT).format(date);
    }

    public static String getCurrentDateDDMMYYString() {
        return getDDMMYYString(new Date());
    }

    public static String getDDMMYYString(Date date) {
        return new SimpleDateFormat(DDMMYY).format(date);
    }

    public static LocalDateTime convertToLocalDateTimeViaInstant(Date dateToConvert) {
        return dateToConvert.toInstant()
                .atZone(ZoneId.systemDefault())
                .toLocalDateTime();
    }

}
