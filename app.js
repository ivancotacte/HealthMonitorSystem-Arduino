import express from 'express';
import bodyParser from 'body-parser';
import mongoose from 'mongoose';
import dotenv from 'dotenv';
import generator from "generate-password";
import expressLayouts from "express-ejs-layouts";

import UserModels from "./models/UserModels.js";

dotenv.config();

mongoose.connect(process.env.MONGO_URI, {
}).then(() => {
    console.log('Connected to the database');
}).catch((err) => {
    console.log(err);
});

const app = express();
const PORT = process.env.PORT || 3000;

app.use(expressLayouts);
app.set("layout", "./layouts/portal_layout.ejs");

app.use(express.static("public"));
app.use("/css", express.static("public/css"));

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');



app.get('/login', (req, res) => {
    res.render('login', { title: 'Login' });
});

app.get("/register", (req, res) => {
  res.render("register", { title: "Login" });
});

app.post("/register", async (req, res) => {
    const { firstName, middleName, lastName, suffix, age, gender, height, emailAddress, contactNumber } = req.body;

    let UserEmail = await UserModels.findOne({ emailAddress });
    let UserConteactNumber = await UserModels.findOne({ contactNumber });

    if (UserEmail) {
        return res.render("register", {
            title: "Register",
            error: "Email already exists",
            success: "",
        });
    } else if (UserConteactNumber) {
        return res.render("register", {
            title: "Register",
            error: "Contact number already exists",
            success: "",
        });
    }

    const randomPassword = generator.generate({ length: 15, numbers: true });

    const user = new UserModels({
        firstName,
        middleName,
        lastName,
        suffix,
        age,
        gender,
        height,
        emailAddress,
        password: randomPassword,
        contactNumber,
    });

    await user.save();
    res.render("register", {
        title: "Register",
        error: "",
        success: "Account created successfully",
    });
});



app.post('/bpm', (req, res) => {
    const { bpm_rate } = req.body;
    console.log(bpm_rate);
});








app.listen(PORT, () => {
    console.log(`Server is running on PORT ${PORT}`);
});