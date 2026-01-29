const express = require("express");
const multer = require("multer");
const fs = require("fs");
const path = require("path");
const os = require("os");

const app = express();
const PORT = 8000;

/* =========================
   GET WIFI IP
========================= */
function getWifiIP() {
    const nets = os.networkInterfaces();
    for (const name of Object.keys(nets)) {
        if (!name.toLowerCase().includes("wi")) continue;
        for (const net of nets[name]) {
            if (net.family === "IPv4" && !net.internal) {
                return net.address;
            }
        }
    }
    return "0.0.0.0";
}

const WIFI_IP = getWifiIP();

/* =========================
   FOLDERS
========================= */
const uploadDir = path.join(__dirname, "uploads");
const publicDir = path.join(__dirname, "public");

if (!fs.existsSync(uploadDir)) fs.mkdirSync(uploadDir);
if (!fs.existsSync(publicDir)) fs.mkdirSync(publicDir);

/* =========================
   MIDDLEWARE
========================= */
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(express.static(publicDir));

/* =========================
   HTML
========================= */
app.get("/", (req, res) => {
    res.sendFile(path.join(publicDir, "index.html"));
});

/* =========================
   MULTER (FILE NAME = USER INPUT)
========================= */
const storage = multer.diskStorage({
    destination: uploadDir,
    filename: (req, file, cb) => {
        cb(null, `${req.body.name}.hex`);
    }
});
const upload = multer({ storage });

/* =========================
   UPLOAD FIRMWARE
========================= */
app.post("/upload", upload.single("firmware"), (req, res) => {
    const { name, version } = req.body;

    if (!name || !version) {
        return res.status(400).json({ ok: false });
    }

    const meta = { name, version };

    fs.writeFileSync(
        path.join(uploadDir, "firmware.json"),
        JSON.stringify(meta, null, 2)
    );

    console.log("📦 Upload OK:", meta);
    res.json({ ok: true, name, version });
});

/* =========================
   GET VERSION
========================= */
app.get("/version", (req, res) => {
    const metaFile = path.join(uploadDir, "firmware.json");
    if (!fs.existsSync(metaFile)) {
        return res.json({ name: "", version: "0.0.0" });
    }
    res.json(JSON.parse(fs.readFileSync(metaFile, "utf8")));
});

/* =========================
   DOWNLOAD FIRMWARE
========================= */
app.get("/firmware", (req, res) => {
    const metaFile = path.join(uploadDir, "firmware.json");
    if (!fs.existsSync(metaFile)) {
        return res.status(404).send("No firmware");
    }

    const { name } = JSON.parse(fs.readFileSync(metaFile, "utf8"));
    const fwPath = path.join(uploadDir, `${name}.hex`);

    if (!fs.existsSync(fwPath)) {
        return res.status(404).send("Firmware not found");
    }

    res.download(fwPath, `${name}.hex`);
});

/* =========================
   OTA STATE (ON / OFF)
========================= */
const otaStateFile = path.join(uploadDir, "ota_state.json");

function readOtaState() {
    if (!fs.existsSync(otaStateFile)) return { ota: false };
    return JSON.parse(fs.readFileSync(otaStateFile, "utf8"));
}

function writeOtaState(state) {
    fs.writeFileSync(otaStateFile, JSON.stringify(state, null, 2));
}

/* GET OTA STATE */
app.get("/status-ota", (req, res) => {
    res.json(readOtaState());
});

/* SET OTA STATE */
app.post("/status-ota", (req, res) => {
    const { ota } = req.body;
    writeOtaState({ ota: !!ota });
    console.log("🔁 OTA STATE =", ota);
    res.json({ ok: true, ota: !!ota });
});

/* =========================
   START SERVER
========================= */
app.listen(PORT, WIFI_IP, () => {
    console.log("🚀 OTA SERVER RUNNING");
    console.log(`WEB      : http://${WIFI_IP}:${PORT}`);
    console.log(`VERSION  : http://${WIFI_IP}:${PORT}/version`);
    console.log(`FIRMWARE : http://${WIFI_IP}:${PORT}/firmware`);
    console.log(`OTA      : http://${WIFI_IP}:${PORT}/ota`);
});
