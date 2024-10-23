import { List, ListItem, ListSubheader, ListItemText, Stack } from "@mui/material"
import {SxProps, useTheme } from "@mui/material"
import {useEffect, useState} from "react"
import rip_gif from "../../assets/skeleton-dancing.gif"
import chad_gif from "../../assets/gigachad.gif"

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

interface Client {
    last_name: String,
    address: String,
    phone_number: String,
    last: Number,
    is_alive: Boolean
}

const requestGetAll = async () => {
    try {
        const response = await (await fetch('http://0.0.0.0:8080/GetAllClients', {
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

const Clients = () => {
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
            <li key={`section-${"names"}`}>
                <ul>
                    <Stack spacing={'1px'} sx={useStyles().stack}>
                        {Items.map((item) => (
                            <Stack spacing={'1px'} direction="row" sx={useStyles().stack}>
                                <ListItem sx={useStyles().cell}>
                                    <ListItemText primary={`${item.last_name}`} />
                                </ListItem>
                                <ListItem sx={useStyles().cell}>
                                    <ListItemText primary={`${item.address}`} />
                                </ListItem>
                                <ListItem sx={useStyles().cell}>
                                    <ListItemText primary={`${item.phone_number}`} />
                                </ListItem>
                                <ListItem sx={useStyles().cell}>
                                    <ListItemText primary={`${item.last}`} />
                                </ListItem>
                                <img src={(item.is_alive ? chad_gif : rip_gif)} alt="loading..." height={100} width={50} />
                            </Stack>
                        ))}
                    </Stack>
                </ul>
            </li>
        </List>
    );
}
export default Clients;