import { List, ListItem, ListSubheader, ListItemText, Stack } from "@mui/material"
import {SxProps, useTheme } from "@mui/material"
import {useEffect, useState} from "react"

const useStyles = () => {
    const theme = useTheme();
    const palette = theme.palette;
    return {
        list: {
            backgroundColor: palette.surface.main,
            width: "100%",
            height: "100%",
            display: "flex",
            flexDirection: "column",
            borderRadius: "0px",
            PaddingBottom: "0px"
        },
        cell: {
            backgroundColor: palette.secondaryContainer.main,
            borderRadius: "2px",
            PendingTop: "0px",
            PaddingBottom: "1px",
            margin: "0px",
        },
        header: {
            backgroundColor: palette.secondaryContainer.main,
            borderRadius: "2px",
            PendingTop: "0px",
            PaddingBottom: "1px",
            margin: "1px",
        },
        stack: {
            PendingTop: "1px",
            PaddingBottom: "1px",
            margin: "1px",
        },
        celltext: {
            alignItems: "center",
        }
    }
}

interface Drug {
    name: String,
    group: String,
    type: String,
    dosage: Number,
    expiration_date: Date,
    quantity: Number,
    retail_price: String
}

interface Client {
    last_name: String,
    address: String,
    phone_number: String,
    drugs: Drug[]
}

const requestGetAll = async () => {
    try {
        const response = await (await fetch('http://0.0.0.0:8080/GetSolveToday', {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json'
            }
        })).json();
        return response as Client[];
    } catch (e) {
        console.log(e);
    }
}

const SolveToday = () => {
    const [Items, setItems] = useState<Client[]>([]);
    useEffect(() => {
        const GetData = async () => {
            const answer = await requestGetAll();
            if (answer != undefined) {
                setItems(answer);
            }
        }
        GetData()
    }, []);
    return (
        <List sx={useStyles().list}>
        {Items.map((client) => (
            <li key={`section-${client.last_name}`}>
              <ul>
                <Stack spacing={'1px'} direction="row" sx={useStyles().stack}>
                    <ListItem sx={useStyles().cell}>
                        <ListItemText primary={`${client.last_name}`} />
                    </ListItem>
                    <ListItem sx={useStyles().cell}>
                        <ListItemText primary={`${client.address}`} />
                    </ListItem>
                    <ListItem sx={useStyles().cell}>
                        <ListItemText primary={`${client.phone_number}`} />
                    </ListItem>
                </Stack>
                <Stack spacing={'1px'} direction="row" sx={useStyles().stack}>
                    <ListItem sx={useStyles().cell}>
                        <ListItemText primary={"Name"} />
                    </ListItem>
                    <ListItem sx={useStyles().cell}>
                        <ListItemText primary={"Dosage"} />
                    </ListItem>
                    <ListItem sx={useStyles().cell}>
                        <ListItemText primary={"Quantity"} />
                    </ListItem>
                    <ListItem sx={useStyles().cell}>
                        <ListItemText primary={"Expiration date"} />
                    </ListItem>
                    <ListItem sx={useStyles().cell}>
                        <ListItemText primary={"Retail prices"} />
                    </ListItem>
                </Stack>
                {client.drugs.map((drug) => (
                    <Stack spacing={'1px'} direction="row" sx={useStyles().stack}>
                        <ListItem sx={useStyles().cell}>
                            <ListItemText primary={`${drug.name}`} />
                        </ListItem>
                        <ListItem sx={useStyles().cell}>
                            <ListItemText primary={`${drug.dosage}`} />
                        </ListItem>
                        <ListItem sx={useStyles().cell}>
                            <ListItemText primary={`${drug.quantity}`} />
                        </ListItem>
                        <ListItem sx={useStyles().cell}>
                            <ListItemText primary={`${drug.expiration_date}`} />
                        </ListItem>
                        <ListItem sx={useStyles().cell}>
                            <ListItemText primary={`${drug.retail_price}`} />
                        </ListItem>
                    </Stack>
                ))}
              </ul>
            </li>
          ))}
        </List>);
}
export default SolveToday;