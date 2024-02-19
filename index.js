const express = require('express');
require('dotenv');

const app = express();
const PORT = process.env.PORT





app.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`)
});