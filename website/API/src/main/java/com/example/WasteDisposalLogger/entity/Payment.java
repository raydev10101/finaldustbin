package com.example.WasteDisposalLogger.entity;

import jakarta.persistence.*;
import lombok.*;

import java.math.BigDecimal;

@Entity
@NoArgsConstructor
@AllArgsConstructor
@Builder
@Data
public class Payment {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Long id;
    private BigDecimal balance;
    @OneToOne(cascade = CascadeType.ALL, fetch = FetchType.EAGER)
    private ApplicationUser appUser;

    public void increase(BigDecimal amount) {
        this.balance = this.balance.add(amount);
    }
    public void decrease(BigDecimal amount) {
        this.balance = this.balance.subtract(amount);
    }
}
