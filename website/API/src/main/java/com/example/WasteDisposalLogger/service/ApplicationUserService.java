package com.example.WasteDisposalLogger.service;

import com.example.WasteDisposalLogger.dto.Login;
import com.example.WasteDisposalLogger.dto.UserDto;
import com.example.WasteDisposalLogger.entity.ApplicationUser;
import com.example.WasteDisposalLogger.repository.ApplicationUserRepository;
import lombok.RequiredArgsConstructor;
import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Service;

import java.util.Optional;
@Service
@RequiredArgsConstructor
public class ApplicationUserService {
    private final ApplicationUserRepository applicationUserRepository;
    private final PaymentService paymentService;

    public int signUp(UserDto userDto){
        Optional<ApplicationUser> optionalApplicationUser = applicationUserRepository.findByUsername(userDto.getUsername());
        if (optionalApplicationUser.isEmpty()){
            applicationUserRepository.save(ApplicationUser.builder()
                    .phoneNumber(userDto.getPhoneNumber())
                    .password(userDto.getPassword())
                    .username(userDto.getUsername())
                    .build());
            try {
                paymentService.createPayment(userDto.getUsername());
            }catch (Exception e){
                e.printStackTrace();
                return HttpStatus.BAD_REQUEST.value();
            }
            return 200;
        }
        return HttpStatus.CONFLICT.value();
    }
    public ApplicationUser login(Login login){
        Optional<ApplicationUser> optionalApplicationUser = applicationUserRepository.findByUsernameAndPassword(login.getUsername(),login.getPassword());
        return optionalApplicationUser.orElse(null);
    }
}
