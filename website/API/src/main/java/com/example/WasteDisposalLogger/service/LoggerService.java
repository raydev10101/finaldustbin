package com.example.WasteDisposalLogger.service;

import com.example.WasteDisposalLogger.constants.ConstantMessages;
import com.example.WasteDisposalLogger.dto.GetLogs;
import com.example.WasteDisposalLogger.dto.SaveLogDTO;
import com.example.WasteDisposalLogger.dto.SaveLogResponse;
import com.example.WasteDisposalLogger.entity.Logger;
import com.example.WasteDisposalLogger.repository.LoggerRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.List;
import java.util.Locale;

@Service
@Slf4j
@RequiredArgsConstructor
public class LoggerService {
    private final LoggerRepository loggerRepository;
    private final PaymentService paymentService;
    private static final DateTimeFormatter DATE_TIME_FORMATTER = DateTimeFormatter.ISO_LOCAL_DATE_TIME;
    public SaveLogResponse saveLog(SaveLogDTO saveLogDTO) throws Exception {
        saveLogDTO.validated();
        ConstantMessages[] messages= ConstantMessages.values();
        for (ConstantMessages cc: messages){
            if (cc.getAsText().equalsIgnoreCase(saveLogDTO.getMessage())){
                if (cc.equals(ConstantMessages.WASTE_BIN_IS_OPEN)){
                    paymentService.DecrementBalance(saveLogDTO.getUsername(), BigDecimal.valueOf(100.00));
                }
                try {
                    LocalDateTime localDateTime = LocalDateTime.parse(saveLogDTO.getDateTime());
                    Logger logData = Logger.builder()
                            .message(cc.getAsText())
                            .dateTime(localDateTime)
                            .build();
                    logData = loggerRepository.save(logData);
                    return SaveLogResponse.builder()
                            .message("SUCCESSFULLY SAVED LOG")
                            .status(true)
                            .data(SaveLogResponse.MetaData.builder()
                                    .savedLogMessage(logData.getMessage())
                                    .savedLogDateTime(logData.getDateTime().toString())
                                    .build())
                            .build();
                }catch (Exception e){
                    log.info("Error",e);
                    return SaveLogResponse.builder()
                            .message("COULD NOT SAVE LOG INVALID DATE TIME ensure it is of the format YYYY-MM-DDThh:mm:ss")
                            .status(false)
                            .build();
                }
            }
         }
        return SaveLogResponse.builder()
                .message("COULD NOT SAVE LOG BAD MESSAGE TYPE")
                .status(false)
                .build();
    }
    public GetLogs getLogs(String dateFrom, String dateTo){
        try {
            LocalDateTime from = LocalDateTime.parse(dateFrom);
            LocalDateTime to = LocalDateTime.parse(dateTo);
            log.info("All {}",loggerRepository.findAll());
            log.info("From {} TO {}",from,to);

            List<Logger> loggers = loggerRepository.findByDateTimeBetween(from, to);
            if (loggers.isEmpty()) {
                return GetLogs.builder()
                        .message("NO LOGS FOUND")
                        .status(false)
                        .build();
            }
            List<GetLogs.MetaData> metaData = loggers.stream().map(logger ->
                    GetLogs.MetaData.builder().logMessage(logger.getMessage()).logDateTime(logger.getDateTime().toString()).build()).toList();

            return GetLogs.builder()
                    .message("Successfully fetched logs".toUpperCase(Locale.ROOT))
                    .status(true)
                    .data(metaData)
                    .build();
        }catch (Exception e){
            return GetLogs.builder()
                    .message("Invalid date time parameters")
                    .status(false)
                    .build();

        }
    }

}
