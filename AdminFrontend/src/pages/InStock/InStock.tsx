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

interface DrugTypeGroup {
    name: String
    data: Drug[]
}

interface DrugGroup {
    name: String
    list: DrugTypeGroup[]
}

const requestGetAll = async () => {
    try {
        const response = await (await fetch('http://0.0.0.0:8080/GetAllDrugs', {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json'
            }
        })).json();
        return response as Drug[];
    } catch (e) {
        console.log(e);
    }
}

const InStock = () => {
    const [Items, setItems] = useState<DrugGroup[]>([]);
    useEffect(() => {
        const GetData = async () => {
            const answer = await requestGetAll();
            if (answer != undefined) {
                const mp = new Map<String, Map<String, Drug[]>>()
                answer.map(element => {
                    if (mp.has(element.group)) {
                        if (mp.get(element.group)?.has(element.type)) {
                            mp.get(element.group)?.get(element.type)?.push(element)
                        } else {
                            mp.get(element.group)?.set(element.type, [ element ])
                        }
                    } else {
                        const newMp = new Map<String, Drug[]>()
                        newMp.set(element.type, [ element ])
                        mp.set(element.group, newMp)
                    }
                });
                const res: DrugGroup[] = [];
                for (let[key, value] of mp) {
                    const drug_type_group: DrugTypeGroup[] = []
                    for (let[gkey, gvalue] of value) {
                        drug_type_group.push({ name: gkey, data: gvalue })
                    }
                    res.push({ name: key, list: drug_type_group })
                }
                setItems(res);
            }
        }
        GetData()
    }, []);
    return (
        <List sx={useStyles().list}>
        {Items.map((drug_group) => (
            <li key={`section-${drug_group.name}`}>
              <ul>
                <ListSubheader sx={useStyles().header} disableSticky>{`${drug_group.name}`}</ListSubheader>
                {drug_group.list.map((drug_type_group) => (
                    <li key={`section-${drug_type_group.name}`}>
                            <ListSubheader sx={useStyles().header} disableSticky>{`${drug_type_group.name}`}</ListSubheader>
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
                            {drug_type_group.data.map((drug) => (
                                <Stack spacing={'1px'} direction="row" sx={useStyles().stack}>
                                    <ListItem key={`item-${drug.name}-${drug.name}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.name}`} />
                                    </ListItem>
                                    <ListItem key={`item-${drug.dosage}-${drug.dosage}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.dosage}`} />
                                    </ListItem>
                                    <ListItem key={`item-${drug.quantity}-${drug.quantity}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.quantity}`} />
                                    </ListItem>
                                    <ListItem key={`item-${drug.expiration_date}-${drug.expiration_date}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.expiration_date}`} />
                                    </ListItem>
                                    <ListItem key={`item-${drug.retail_price}-${drug.retail_price}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.retail_price}`} />
                                    </ListItem>
                                </Stack>
                            ))}
                    </li>
                ))}
              </ul>
            </li>
          ))}
        </List>
    );
}
export default InStock;