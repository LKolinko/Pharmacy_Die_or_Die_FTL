import { QrCode } from "@mui/icons-material";
import { Button, Stack, Typography, Box, TextField } from "@mui/material";
import { useState } from "react";

interface Drug {
    name: String,
    group: String,
    type: String,
    dose: String,
    expiryDate: String,
    quantity: String,
    wholesalePrice: String,
    retailPrice: String
}

const requestAdd = async (drug: Drug): Promise<any> => {
    try {
        const response = await fetch('http://0.0.0.0:3000/addDrug', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(drug) || null
        });
        return await response.json();
    } catch (e) {
        console.log(e);
    }
}

const AddItem = () => {
    const [Name, setName] = useState('')
    const [Dosage, setDosage] = useState('')
    const [Type, setType] = useState('')
    const [ExpirationDate, setExpirationDate] = useState('')
    const [WholesalePrice, setWholesalePrice] = useState('')
    const [RetailPrice, setRetailPrice] = useState('')
    const [DrugGroup, setDrugGroup] = useState('')
    const [DrugQuantity, setDrugQuantity] = useState('')
    const handleNameChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setName(event.target.value);
    }
    const handleDosageChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setDosage(event.target.value);
    }
    const handleTypeChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setType(event.target.value);
    }
    const handleExpirationDateChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setExpirationDate(event.target.value);
    }
    const handleWholesalePriceChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setWholesalePrice(event.target.value);
    }
    const handleRetailPriceChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setRetailPrice(event.target.value);
    }
    const handleDrugGroupChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setDrugGroup(event.target.value);
    }
    const handleDrugQuantityChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setDrugQuantity(event.target.value);
    }
    return (
        <Stack spacing={3}>
            <TextField variant="outlined" id="Name" label="Drug name" value={Name} onChange={handleNameChange}/>
            <TextField variant="outlined" id="Dosage" label="Dosage" value={Dosage} onChange={handleDosageChange}/>
            <TextField variant="outlined" id="Type" label="Type" value={Type} onChange={handleTypeChange}/>
            <TextField variant="outlined" id="ExpirationDate" label="Expiration date" value={ExpirationDate} onChange={handleExpirationDateChange}/>
            <TextField variant="outlined" id="WholesalePrice" label="Wholesale price" value={WholesalePrice} onChange={handleWholesalePriceChange}/>
            <TextField variant="outlined" id="RetailPrice" label="Retail price" value={RetailPrice} onChange={handleRetailPriceChange}/>
            <TextField variant="outlined" id="DrugGroup" label="Drug group" value={DrugGroup} onChange={handleDrugGroupChange}/>
            <TextField variant="outlined" id="DrugQuantity" label="Drug quantity" value={DrugQuantity} onChange={handleDrugQuantityChange}/>
            <Button variant="filled" onClick={ async () => {
                const answer = (await requestAdd({ name: Name,
                            group: DrugGroup,
                            type: Type,
                            dose: Dosage,
                            expiryDate: ExpirationDate,
                            quantity: DrugQuantity,
                            wholesalePrice: WholesalePrice,
                            retailPrice: RetailPrice})).answer
                alert(answer)
            }}>Add Drug</Button>
        </Stack>
    );
}
export default AddItem;