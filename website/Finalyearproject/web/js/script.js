import axios, {isCancel, AxiosError} from 'axios';
import axios from 'axios';
const {Client} = require("pg"); 

const client = new Client({
    host:"localhost",
    user: "samuel",
    port: 5432,
    password: "password",
    database: "waste_disposal_logger"

})
client.connect();

client.query('select * from logger',(err, res)=>{
    if(!err){
        console.log(res.rows);

    }
    else{
        console.log("Error Occured: "+ err.message);
    }
    client.end;
})



// document.querySelector('.signup-form').addEventListener('submit', function(event) {
//     event.preventDefault();
    
//     const fullname = document.getElementById('fullname').value.trim();
//     const phone = document.getElementById('phone').value.trim();
//     const password = document.getElementById('password').value.trim();
//     const confirmPassword = document.getElementById('confirm-password').value.trim();
    
//     if (!fullname || !phone || !password || !confirmPassword) {
//         alert('All fields are required!');
//         return;
//     }
//     if (password !== confirmPassword) {
//         alert('Passwords do not match!');
//     } else {
//         alert('Signup successful!');
//         // You can add more functionality here, such as submitting the form data to the server.
//     }
//     document.getElementById("redirect").addEventListener("click", redirect);
//     function redirect(){
//         window.location="Users.html"; 
//     }
// });
