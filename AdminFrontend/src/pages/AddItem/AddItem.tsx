import { Button, Stack, Typography, Box, TextField, Alert } from "@mui/material";
import React, { useState, useRef } from 'react';
import CheckIcon from '@mui/icons-material/Check';

const AddItem = () => {
    return (
        <Stack spacing={3}>
            <TextField variant="outlined" id="Name" label="Drug name"/>
            <TextField variant="outlined" id="Dosage" label="Dosage"/>
            <TextField variant="outlined" id="Type" label="Type"/>
            <TextField variant="outlined" id="ExpirationDate" label="Expiration date"/>
            <TextField variant="outlined" id="WholesalePrice" label="Wholesale price"/>
            <TextField variant="outlined" id="DrugGroup" label="Drug group"/>
            <TextField variant="outlined" id="DrugQuantity" label="Drug quantity"/>
            <Button variant="filled">Add Drug</Button>
        </Stack>
    );
}
export default AddItem;