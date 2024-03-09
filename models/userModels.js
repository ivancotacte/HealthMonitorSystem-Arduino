import mongoose from "mongoose";
const Schema = mongoose.Schema;

const UserSchema = new Schema({
  IDNumber: {
    type: String,
    required: true,
    unique: true,
    default: () => {
      const school = "HMS";
      const randomDigits = Math.floor(1000000000 + Math.random() * 9000000000);
      return `${school}${randomDigits}`;
    },
  },
  firstName: {
    type: String,
    required: true,
  },
  middleName: {
    type: String,
    required: false,
  },
  lastName: {
    type: String,
    required: true,
  },
  suffix: {
    type: String,
    required: false,
  },
  age: {
    type: Number,
    required: true,
  },
  gender: {
    type: String,
    required: true,
  },
  height: {
    type: String,
    required: true,
  },
  weight: {
    type: String,
    required: false,
  },
  heartRate: {
    type: String,
    required: false,
  },
  emailAddress: {
    type: String,
    required: true,
  },
  password: {
    type: String,
    required: true,
  },
  contactNumber: {
    type: String,
    required: true,
  },
});

export default mongoose.model("User", UserSchema);