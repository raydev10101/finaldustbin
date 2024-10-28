package com.example.WasteDisposalLogger.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import lombok.NonNull;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class SaveLogDTO {
    private String dateTime;
    private String message;
    private String username;


    public void validated() throws Exception {
        if (dateTime == null || dateTime.isEmpty() || message == null || message.isEmpty() || username == null || username.isEmpty() ){
            throw new Exception("ALL FIELDS ARE NON NULLABLE");
        }
    }

}
