// Importamos las librerías necesarias
const express = require("express");  // Framework para manejar peticiones HTTP
const mongoose = require("mongoose");  // Librería para conectarse y manejar MongoDB
const cors = require("cors");  // Middleware para permitir solicitudes desde otros dominios (CORS)

// Conectar a MongoDB Atlas y seleccionar la base de datos ProyectoSensor
mongoose.connect("mongodb+srv://42202377:luan2004plsa@luis.v46eh.mongodb.net/ProyectoSensor?retryWrites=true&w=majority&appName=Luis", {
    useNewUrlParser: true,  // Usa el nuevo parser de URL de MongoDB
    useUnifiedTopology: true // Usa el nuevo motor de monitoreo de servidores
})
.then(() => console.log("Conectado a MongoDB Atlas"))  // Mensaje si la conexión es exitosa
.catch((err) => console.error("Error al conectar a MongoDB:", err)); // Mensaje en caso de error

// Creamos una aplicación Express
const app = express();

// Middleware para permitir solicitudes desde otros dominios (CORS)
app.use(cors());

// Middleware para procesar datos en formato JSON en las peticiones HTTP
app.use(express.json()); 

// Definir el esquema (estructura) de los documentos en la colección SensorProximidad
const sensorSchema = new mongoose.Schema({
    sensorKey: String,  // Identificador del sensor
    distance: Number,  // Distancia medida por el sensor
    timestamp: String,  // Marca de tiempo del dato recibido
    latitude: Number,  // Coordenada de latitud
    longitude: Number  // Coordenada de longitud
});

// Definir el modelo basado en el esquema para interactuar con MongoDB
// El tercer parámetro "SensorProximidad" especifica el nombre exacto de la colección en la base de datos
const SensorProximidad = mongoose.model("SensorProximidad", sensorSchema, "SensorProximidad");

// Ruta para recibir datos del ESP32 mediante una solicitud HTTP POST
app.post('/api/sensor', async (req, res) => {
    // Extraer datos del cuerpo de la petición
    const { sensorKey, distance, timestamp, latitude, longitude } = req.body;
    
    // Crear una nueva instancia del modelo con los datos recibidos
    const sensorData = new SensorProximidad({
        sensorKey,
        distance,
        timestamp,
        latitude,
        longitude
    });
    
    try {
        // Guardar los datos en la base de datos
        const result = await sensorData.save();
        console.log('Documento insertado:', result);
        res.status(201).send('Datos recibidos y guardados correctamente'); // Respuesta al cliente
    } catch (error) {
        console.error('Error al guardar los datos:', error);
        res.status(500).send('Error al guardar los datos'); // Respuesta en caso de error
    }
});

// Definir el puerto donde escuchará el servidor
const PORT = 3000;
app.listen(PORT, () => {
    console.log(`Servidor corriendo en http://localhost:${PORT}`); // Mensaje en consola al iniciar el servidor
});

