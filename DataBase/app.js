const mongoose = require('mongoose');
const express = require('express');
const cors = require('cors');
const app = express();

app.use(express.json());
app.use(cors());

mongoose.connect('mongodb://root:31415981@localhost:27017/pharmacyDB?authSource=admin', {
    useNewUrlParser: true,
    useUnifiedTopology: true
})
.then(() => console.log('Подключение к базе данных установлено'))
.catch(err => console.error('Ошибка подключения:', err));

const drugSchema = new mongoose.Schema({
    name: String,
    group: String,
    type: String,
    dose: Number,
    expiryDate: Date,
    quantity: Number,
    wholesalePrice: Number,
    retailPrice: Number
});

const Drug = mongoose.model('Drug', drugSchema);

app.post('/addDrug', async (req, res) => {
    const { name, group, type, dose, expiryDate, quantity, wholesalePrice, retailPrice } = req.body;

    try {
        const drug = await Drug.findOne({ name, group, type, dose, expiryDate });

        if (drug) {
            drug.quantity += quantity;
            await drug.save();
            res.send('The number of drugs has been updated.');
        } else {
            const newDrug = new Drug({
                name,
                group,
                type,
                dose,
                expiryDate,
                quantity,
                wholesalePrice,
                retailPrice
            });

            await newDrug.save();
            res.send('New medicine added.');
        }
    } catch (err) {
        res.status(500).send('Error adding medicine: ' + err.message);
    }
});

app.delete('/deleteDrug', async (req, res) => {
    const { name, group, type, dose, expiryDate } = req.body;

    try {
        const result = await Drug.deleteOne({ name, group, type, dose, expiryDate });
        if (result.deletedCount > 0) {
            res.send('The medicine has been removed.');
        } else {
            res.status(404).send('No cure found.');
        }
    } catch (err) {
        res.status(500).send('Error when deleting medication: ' + err.message);
    }
});

app.get('/getAllDrugs', async (req, res) => {
    try {
        const drugs = await Drug.find();
        res.json(drugs);
    } catch (err) {
        res.status(500).send('Error while retrieving a list of medications: ' + err.message);
    }
});

const PORT = 3000;
app.listen(PORT, () => {
    console.log(`Сервер запущен на порту ${PORT}`);
});
