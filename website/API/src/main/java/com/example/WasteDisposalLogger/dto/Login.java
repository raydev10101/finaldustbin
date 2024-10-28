package com.example.WasteDisposalLogger.dto;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import lombok.Data;
import lombok.NonNull;

@JsonIgnoreProperties(ignoreUnknown = true)
@Data
public class Login {
    @NonNull
    private String username;
    @NonNull
    private String password;
}
