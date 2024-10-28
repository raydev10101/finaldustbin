package com.example.WasteDisposalLogger.repository;

import com.example.WasteDisposalLogger.entity.ApplicationUser;
import com.twilio.rest.microvisor.v1.App;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface ApplicationUserRepository extends JpaRepository<ApplicationUser, Integer> {
    Optional<ApplicationUser> findByUsernameAndPassword(String username, String password);
    Optional<ApplicationUser> findByUsername(String username);
}
