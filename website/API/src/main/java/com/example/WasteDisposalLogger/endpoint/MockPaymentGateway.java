package com.example.WasteDisposalLogger.endpoint;

import com.example.WasteDisposalLogger.service.PaymentService;
import com.fasterxml.jackson.core.JsonProcessingException;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.math.BigDecimal;

@RestController
@RequestMapping("api/payment-gateway")
@RequiredArgsConstructor
public class MockPaymentGateway {
    private final PaymentService paymentService;

    @PostMapping("/increment")
    public ResponseEntity<String> a(@RequestParam String username, @RequestParam String amount) throws JsonProcessingException {
        return paymentService.incrementBalance(username, BigDecimal.valueOf(Double.parseDouble(amount)));
    }
    @PostMapping
    public ResponseEntity<String> b (@RequestParam String username) throws JsonProcessingException {
        return paymentService.createPayment(username);
    }
    @GetMapping
    public ResponseEntity<String> c(@RequestParam String username) throws JsonProcessingException {
        return paymentService.fetchMyBalance(username);
    }
    @PostMapping("/decrease")
    public ResponseEntity<String> d(@RequestParam String username, @RequestParam String amount) throws JsonProcessingException {
        return paymentService.DecrementBalance(username, BigDecimal.valueOf(Double.parseDouble(amount)));
    }

}
