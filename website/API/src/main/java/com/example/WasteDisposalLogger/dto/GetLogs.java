package com.example.WasteDisposalLogger.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;
@NoArgsConstructor
@AllArgsConstructor
@Builder
@Data
public class GetLogs {
    private boolean status;
    private String message;
    private List<MetaData> data;

    @Data
    @NoArgsConstructor
    @AllArgsConstructor
    @Builder
    public static class MetaData{
        private String logMessage;
        private String logDateTime;
    }
}
