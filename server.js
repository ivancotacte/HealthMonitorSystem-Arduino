import express from 'express';
import bodyParser from 'body-parser';
import mongoose from 'mongoose';  
import dotenv from 'dotenv';
import expressLayouts from 'express-ejs-layouts';
import session from 'express-session';
import MongoDBSession from "connect-mongodb-session";

import UserModels from './models/UserModels.js';

dotenv.config();

const app = express(); 
const PORT = process.env.PORT || 3000;
const MongoDBStore = MongoDBSession(session);
const store = new MongoDBStore({
  uri: process.env.MONGODB_URI,
  collection: process.env.MONGO_COLLECTION,
  databaseName: process.env.MONGO_DB,
});

mongoose.connect(process.env.MONGODB_URI, {
  dbName: process.env.MONGO_DB,
})
.then(() => {console.log('Connected to database');})
.catch((err) => {console.log('Error: ', err);});

app.set("view engine", "ejs");
app.use(express.static("public"));
app.use(expressLayouts);
app.set("layout", "layouts/main_layout.ejs");


app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

app.get('/', (req, res) => {
  res.render("login", { title: "Login" });
});

app.get("/dashboard", (req, res) => {
  res.render("dashboard", {
    title: "Dashbaord",
    layout: "layouts/dashboard_layout.ejs",
  });
});

app.get("/login", (req, res) => {
    res.render("login", { title: "Login" } );
});

app.post("/login", async (req, res) => {
  const { emailAddress, password } = req.body;

  let UserEmail = await UserModels.findOne({ emailAddress });

  if (!UserEmail || UserEmail.password !== password) {
    return res.render("login", {
      title: "Login",
      error: "Invalid email or password",
      success: "",
    });
  }

  res.redirect("/dashboard");
});


app.get("/register", (req, res) => {
    res.render("register", { title: "Register" });
});

app.post("/register", async (req, res) => {
  const { firstName, middleName, lastName, suffix, age, gender, height, emailAddress, contactNumber } = req.body;

  let UserEmail = await UserModels.findOne({ emailAddress });
  let UserContactNumber = await UserModels.findOne({ contactNumber });

  if (UserEmail) {
    return res.render("register", {
      title: "Register",
      error: "Email already exists",
      success: "",
    });
  } else if (UserContactNumber) {
    return res.render("register", {
      title: "Register",
      error: "Contact number already exists",
      success: "",
    });
  }
    const newUser = new UserModels({
      firstName,
      middleName,
      lastName,
      suffix,
      age,
      gender,
      height,
      emailAddress,
      password: "password",
      contactNumber
    });

    await newUser.save();
    res.render("register", {
      title: "Register",
      error: "",
      success: "Registered successfully",
    });
});











app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});