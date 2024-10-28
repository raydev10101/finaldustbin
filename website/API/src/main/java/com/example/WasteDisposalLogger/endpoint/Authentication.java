package com.example.WasteDisposalLogger.endpoint;

import com.example.WasteDisposalLogger.dto.Login;
import com.example.WasteDisposalLogger.dto.UserDto;
import com.example.WasteDisposalLogger.entity.ApplicationUser;
import com.example.WasteDisposalLogger.service.ApplicationUserService;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("api/auth")
@RequiredArgsConstructor
public class Authentication {
    private final ApplicationUserService applicationUserService;

    @PostMapping("/login")
    public ResponseEntity<ApplicationUser>  login(@RequestBody Login login){
        ApplicationUser applicationUser = applicationUserService.login(login);
        if (applicationUser == null){
            return ResponseEntity.notFound().build();
        }
        return ResponseEntity.ok(applicationUser);
    }
    @PostMapping("/signup")
    public ResponseEntity<Void> signUp(@RequestBody UserDto userDto){
        int response = applicationUserService.signUp(userDto);
        return response == 200 ?ResponseEntity.ok(null):ResponseEntity.badRequest().build();
    }




}
