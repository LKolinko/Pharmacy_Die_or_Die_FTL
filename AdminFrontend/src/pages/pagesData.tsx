import { routerType } from "../types/router.types";
import ColorSystem from "./ColorSystem/ColorSystem";
import AddItem from "./AddItem/AddItem";

const pagesData: routerType[] = [
    {
        path: "ColorSystem",
        element: <ColorSystem />,
        title: "Color System"
    },
    {
        path: "AddItem",
        element: <AddItem />,
        title: "Add Item"
    },
];

export default pagesData;