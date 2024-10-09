import { Button, Stack, Typography, Box, TextField } from "@mui/material";
import { useState } from "react";

interface GenerationData {
    days: Number
    drugs: Number
    courier: Number
}

const requestNewGeneration = async (data: GenerationData): Promise<any> => {
    try {
        const response = await fetch('http://0.0.0.0:5252/NewGeneration', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(data) || null
        });
        return await response.json();
    } catch (e) {
        console.log(e);
    }
}

const requestNextDay = async (num: Number): Promise<any> => {
    try {
        const response = await fetch('http://0.0.0.0:5252/NextDay', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(num) || null
        });
        return await response.json();
    } catch (e) {
        console.log(e);
    }
}

const Generation = () => {
    const [days, setdays] = useState('')
    const [drugs, setdrugs] = useState('')
    const [courier, setcourier] = useState('')
    const handleDaysChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setdays(event.target.value);
    }
    const handledrugsChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setdrugs(event.target.value);
    }
    const handlecourierChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setcourier(event.target.value);
    }
    return (
        <Stack spacing={3}>
            <TextField variant="outlined" label="Days" value={days} type="number" onChange={handleDaysChange}/>
            <TextField variant="outlined" label="Drugs" value={drugs} type="number" onChange={handledrugsChange}/>
            <TextField variant="outlined" label="Couriers" value={courier} type="number" onChange={handlecourierChange}/>
            <Button variant="filled" onClick={ async () => {
                const answer = (await requestNewGeneration({
                    days: Number(days),
                    drugs: Number(drugs),
                    courier: Number(courier)
                })).answer
                alert(answer)
            }}>New Generation</Button>
            <Button variant="filled" onClick={ async () => {
                const answer = (await requestNextDay(1)).answer
                alert(answer)
            }}>Next Day</Button>
        </Stack>
    )
}
export default Generation;