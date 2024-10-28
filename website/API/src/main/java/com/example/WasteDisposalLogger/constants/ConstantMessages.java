package com.example.WasteDisposalLogger.constants;

import lombok.Getter;

@Getter
public enum ConstantMessages {
    WASTE_BIN_IS_FULL("WASTE BIN IS FULL"),
    WASTE_BIN_IS_OPEN("WASTE BIN IS OPEN"),
    WASTE_BIN_IS_CLOSED("WASTE BIN IS CLOSED"),
    WASTE_BIN_IS_EMPTY("WASTE BIN IS EMPTY");

    private final String asText;
    ConstantMessages(String message){
        asText = message;
    }

}
