package com.example.WasteDisposalLogger.dto;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import lombok.Data;
import lombok.NonNull;

@JsonIgnoreProperties(ignoreUnknown = true)
@Data
public class UserDto {
    @NonNull
    private String phoneNumber;
    @NonNull
    private String username;
    @NonNull
    private String password;

}
