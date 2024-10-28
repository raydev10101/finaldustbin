package com.example.WasteDisposalLogger.service;

import com.example.WasteDisposalLogger.entity.ApplicationUser;
import com.example.WasteDisposalLogger.entity.Payment;
import com.example.WasteDisposalLogger.repository.ApplicationUserRepository;
import com.example.WasteDisposalLogger.repository.PaymentRepository;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Service;

import java.math.BigDecimal;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

@RequiredArgsConstructor
@Service
public class PaymentService {
    private final PaymentRepository paymentRepository;
    private final ApplicationUserRepository applicationUserRepository;
    private final ObjectMapper objectMapper =new ObjectMapper();


    public ResponseEntity<String> createPayment(String userName) throws JsonProcessingException {
        Optional<ApplicationUser> applicationUserOptional =  applicationUserRepository.findByUsername(userName);
        Map<String,String> response = new HashMap<>();
        if (applicationUserOptional.isEmpty()) {
            response.put("message", "User not found");
            return ResponseEntity.badRequest().body(objectMapper.writeValueAsString(response));
        }
        var paymentOptional = paymentRepository.findByAppUser(applicationUserOptional.get());
        if (paymentOptional.isEmpty()) {
            paymentRepository.save(Payment.builder()
                    .balance(BigDecimal.ZERO)
                    .appUser(applicationUserOptional.get()).build());
            response.put("message", "Payment creation successful");
            return ResponseEntity.ok().body(objectMapper.writeValueAsString(response));
        }
        else {
            response.put("message", "Payment Already Exists");
            return ResponseEntity.badRequest().body(objectMapper.writeValueAsString(response));
        }
    }
    public ResponseEntity<String> fetchMyBalance(String userName) throws JsonProcessingException {
        Optional<ApplicationUser> applicationUserOptional =  applicationUserRepository.findByUsername(userName);
        Map<String,Object> response = new HashMap<>();
        if (applicationUserOptional.isEmpty()) {
            response.put("message", "User not found");
            return ResponseEntity.badRequest().body(objectMapper.writeValueAsString(response));
        }
        var paymentOptional = paymentRepository.findByAppUser(applicationUserOptional.get());
        if (paymentOptional.isEmpty()) {
            paymentRepository.save(Payment.builder()
                    .balance(BigDecimal.ZERO)
                    .appUser(applicationUserOptional.get()).build());
            response.put("message", "NO BALANCE FOUND CREATE PAYMENT");
            return ResponseEntity.badRequest().body(objectMapper.writeValueAsString(response));
        }
        else {
            Payment payment = paymentOptional.get();
            response.put("message", "Fetched");
            response.put("balance", payment.getBalance().doubleValue());
            return ResponseEntity.ok().body(objectMapper.writeValueAsString(response));
        }
    }
    public ResponseEntity<String> incrementBalance(String userName, BigDecimal amount) throws JsonProcessingException {
        Optional<ApplicationUser> applicationUserOptional =  applicationUserRepository.findByUsername(userName);
        Map<String,String> response = new HashMap<>();
        if (applicationUserOptional.isEmpty()) {
            response.put("message", "User not found");
            return ResponseEntity.badRequest().body(objectMapper.writeValueAsString(response));
        }
        var paymentOptional = paymentRepository.findByAppUser(applicationUserOptional.get());
        if (paymentOptional.isEmpty()) {
            paymentRepository.save(Payment.builder()
                            .balance(amount)
                            .appUser(applicationUserOptional.get()).build());
            response.put("message", "Payment successful");
            return ResponseEntity.ok().body(objectMapper.writeValueAsString(response));
        }
        Payment payment = paymentOptional.get();
        payment.increase(amount);
        paymentRepository.save(payment);
        response.put("message", "Payment successful");
        return ResponseEntity.ok().body(objectMapper.writeValueAsString(response));
    }
    public ResponseEntity<String> DecrementBalance(String userName, BigDecimal amount) throws JsonProcessingException {
        Optional<ApplicationUser> applicationUserOptional =  applicationUserRepository.findByUsername(userName);
        Map<String,String> response = new HashMap<>();
        if (applicationUserOptional.isEmpty()) {
            response.put("message", "User not found");
            return ResponseEntity.badRequest().body(objectMapper.writeValueAsString(response));
        }
        var paymentOptional = paymentRepository.findByAppUser(applicationUserOptional.get());
        if (paymentOptional.isEmpty()) {
            paymentRepository.save(Payment.builder()
                    .balance(BigDecimal.ZERO)
                    .appUser(applicationUserOptional.get()).build());
            response.put("message", "Your Balance is Insufficient");
            return ResponseEntity.badRequest().body(objectMapper.writeValueAsString(response));
        }
        Payment payment = paymentOptional.get();
        if(payment.getBalance().compareTo(amount) >= 0 ){
            payment.decrease(amount);
        }
        else {
            response.put("message", "Insufficient Balance");
            return ResponseEntity.badRequest().body(objectMapper.writeValueAsString(response));
        }
        paymentRepository.save(payment);
        response.put("message", "Successful");
        return ResponseEntity.ok().body(objectMapper.writeValueAsString(response));
    }


}
