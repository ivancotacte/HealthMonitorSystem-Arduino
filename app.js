import express from 'express';
import bodyParser from 'body-parser';
import mongoose from 'mongoose';
import dotenv from 'dotenv';
import expressLayouts from "express-ejs-layouts";

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
app.listen(PORT, () => {
    console.log(`Server is running on PORT ${PORT}`);
});