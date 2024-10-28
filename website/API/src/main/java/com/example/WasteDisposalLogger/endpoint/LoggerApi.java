package com.example.WasteDisposalLogger.endpoint;

import com.example.WasteDisposalLogger.dto.GetLogs;
import com.example.WasteDisposalLogger.dto.SaveLogDTO;
import com.example.WasteDisposalLogger.dto.SaveLogResponse;
import com.example.WasteDisposalLogger.service.LoggerService;
import lombok.RequiredArgsConstructor;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("api/log")
@RequiredArgsConstructor
public class LoggerApi {
    private final LoggerService loggerService;
    @PostMapping
    public ResponseEntity<SaveLogResponse> saveLog(@RequestBody SaveLogDTO saveLogDTO){
        try {
            SaveLogResponse response = loggerService.saveLog(saveLogDTO);
            return response.isStatus() ? ResponseEntity.ok(response) : ResponseEntity.badRequest().body(response);
        }catch (Exception e){
            return ResponseEntity.badRequest().body(new SaveLogResponse(false,e.getMessage(),null));
        }
    }
    @GetMapping
    public ResponseEntity<GetLogs> getLog(@RequestParam String dateTimeFrom, @RequestParam String dateTimeTo ){
        GetLogs getLogs = loggerService.getLogs(dateTimeFrom,dateTimeTo);
        return getLogs.isStatus()?ResponseEntity.ok(getLogs):ResponseEntity.badRequest().body(getLogs);
    }

}
