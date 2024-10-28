package com.example.WasteDisposalLogger.repository;

import com.example.WasteDisposalLogger.entity.ApplicationUser;
import com.example.WasteDisposalLogger.entity.Payment;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface PaymentRepository extends JpaRepository<Payment, Long> {
    Optional<Payment> findByAppUser(ApplicationUser applicationUser);
}
