document.querySelector('.signup-form').addEventListener('submit', function(event) {
    event.preventDefault();
    
    const fullname = document.getElementById('fullname').value.trim();
    const phone = document.getElementById('phone').value.trim();
    const password = document.getElementById('password').value.trim();
    const confirmPassword = document.getElementById('confirm-password').value.trim();
    
    if (!fullname || !phone || !password || !confirmPassword) {
        alert('All fields are required!');
        return;
    }
    if (password !== confirmPassword) {
        alert('Passwords do not match!');
    } else {
        alert('Signup successful!');
        // You can add more functionality here, such as submitting the form data to the server.
    }
});
