import { Button, Stack, Typography, Box, TextField, LinearProgress } from "@mui/material";
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
    const response = await fetch('http://0.0.0.0:5252/NextDay', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(num) || null
    });
    return await response.json();
}

const Generation = () => {
    const [days, setdays] = useState('')
    const [drugs, setdrugs] = useState('')
    const [courier, setcourier] = useState('')
    const [progress, setprogress] = useState(0);
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
            <TextField variant="outlined" label="Days" value={days} type="number" onChange={handleDaysChange}/>
            <Button variant="filled" onClick={ async () => {
                setprogress(0);
                for (let i = 0; i < Number(days); ++i) {
                    setprogress((i + 1) * 100 / Number(days));
                    try {
                        await requestNextDay(1)
                        console.log(i + " ok")
                    } catch (error) {
                        console.log(error)
                        console.log(i + " repeat")
                        --i;
                    }             
                }
                alert("solve " + days + " days")
            }}>Solve {days} days</Button>
            <LinearProgress variant="determinate" value={progress} />
        </Stack>
    )
}
export default Generation;