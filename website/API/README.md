
# Data Persistence API

This API allows for user authentication, logging of events, and payment gateway functionalities.

## Authentication

### Register User

- **Endpoint**: `POST http://localhost:8080/api/auth/signup`
- **Description**: Registers a new user with phone number, username, and password.

#### Request Body (JSON):
```json
{
    "phoneNumber": "09028133920",
    "username": "superUser",
    "password": "defaultPassword"
}
```

#### Example cURL Request:
```bash
curl --location 'http://localhost:8080/api/auth/signup' --data '{
    "phoneNumber": "09028133920",
    "username": "superUser",
    "password": "defaultPassword"
}'
```

#### Response:
- **Status**: `200 OK`
- **Body**: No response body.

---

### User Login

- **Endpoint**: `POST http://localhost:8080/api/auth/login`
- **Description**: Logs in the user with username and password.

#### Request Body (JSON):
```json
{
    "username": "superUser",
    "password": "defaultPassword"
}
```

#### Example cURL Request:
```bash
curl --location 'http://localhost:8080/api/auth/login' --data '{
    "username": "superUser",
    "password": "defaultPassword"
}'
```

#### Response:
- **Status**: `200 OK`
- **Body (JSON)**:
```json
{
    "userId": 1,
    "username": "superUser",
    "password": "defaultPassword",
    "phoneNumber": "09028133920"
}
```

---

## Logging

### Add Log

- **Endpoint**: `POST http://localhost:8080/api/log`
- **Description**: Logs a user message with a timestamp.

#### Request Body (JSON):
```json
{
    "username": "superUser",
    "message": "WASTE BIN IS OPEN",
    "dateTime": "2024-10-24T19:39:00"
}
```

#### Example cURL Request:
```bash
curl --location 'http://localhost:8080/api/log' --data '{
    "username": "superUser",
    "message": "WASTE BIN IS CLOSED",
    "dateTime": "2024-10-24T19:39:00"
}'
```

#### Response:
- **Status**: `200 OK`
- **Body (JSON)**:
```json
{
    "status": true,
    "message": "SUCCESSFULLY SAVED LOG",
    "data": {
        "savedLogMessage": "WASTE BIN IS CLOSED",
        "savedLogDateTime": "2024-10-24T19:39"
    }
}
```

---

### Fetch Logs

- **Endpoint**: `GET http://localhost:8080/api/log`
- **Description**: Fetches logs within a specific date range.

#### Query Parameters:
- `dateTimeFrom`: Start of the date range (e.g., `2024-01-01T00:00:00`).
- `dateTimeTo`: End of the date range (e.g., `2024-12-24T00:00:00`).

#### Example cURL Request:
```bash
curl --location 'http://localhost:8080/api/log?dateTimeFrom=2024-01-01T00%3A00%3A00&dateTimeTo=2024-12-24T00%3A00%3A00'
```

#### Response:
- **Status**: `200 OK`
- **Body (JSON)**:
```json
{
    "status": true,
    "message": "SUCCESSFULLY FETCHED LOGS",
    "data": [
        {
            "logMessage": "WASTE BIN IS OPEN",
            "logDateTime": "2024-10-24T19:39"
        }
    ]
}
```

---

## Payment Gateway

### Check Balance

- **Endpoint**: `GET http://localhost:8080/api/payment-gateway`
- **Description**: Fetches the user's balance.

#### Query Parameter:
- `username`: The user's username (e.g., `superUser`).

#### Example cURL Request:
```bash
curl --location 'http://localhost:8080/api/payment-gateway?username=superUser'
```

#### Response:
- **Status**: `200 OK`
- **Body (JSON)**:
```json
{
    "balance": 0.0,
    "message": "Fetched"
}
```

---

### Increase Balance

- **Endpoint**: `POST http://localhost:8080/api/payment-gateway/increment`
- **Description**: Increases the user's balance by a specific amount.

#### Query Parameters:
- `username`: The user's username (e.g., `superUser`).
- `amount`: The amount to be added (e.g., `1000`).

#### Example cURL Request:
```bash
curl --location 'http://localhost:8080/api/payment-gateway/increment?username=superUser&amount=1000'
```
