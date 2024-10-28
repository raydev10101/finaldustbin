package com.example.WasteDisposalLogger.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class SaveLogResponse{
    private boolean status;
    private String message;
    private MetaData data;
    @Data
    @NoArgsConstructor
    @AllArgsConstructor
    @Builder
    public static class MetaData{
        private String savedLogMessage;
        private String savedLogDateTime;
    }
}