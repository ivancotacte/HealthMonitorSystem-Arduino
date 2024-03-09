import nodemailer from "nodemailer";

const transporter = nodemailer.createTransport({
  service: "gmail",
  port: "465",
  secure: true,
  logger: false,
  debug: false,
  secureConnection: true,
  auth: {
    user: process.env.GMAIL_EMAIL,
    pass: process.env.GMAIL_PASSWORD,
  },
  tls: {
    rejectUnauthorized: false,
  },
});

function sendEmail(emailAddress, password) {
  const mailOptions = {
    from: '"GROUP 10 - LFSA322N002 👻" <cotactearmenion@gmail.com>',
    to: emailAddress,
    subject: "Your Temporary Password",
    html: `<p>Hello,</p>
         <p>Your temporary password is: <strong>${password}</strong></p>
         <p>Please login and change your password after logging in.</p>`,
  };

  transporter.sendMail(mailOptions, (error, info) => {
    if (error) {
      console.error("Error: ", error);
    } else {
      console.log("Success: ", info.response);
    }
  });
}

export { sendEmail };
