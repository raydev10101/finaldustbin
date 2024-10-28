package com.example.WasteDisposalLogger.repository;

import com.example.WasteDisposalLogger.entity.Logger;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.time.LocalDateTime;
import java.util.List;

@Repository
public interface LoggerRepository extends JpaRepository<Logger, Long> {
    List<Logger> findByDateTimeBetween(LocalDateTime startDateTime, LocalDateTime endDateTime);
}
