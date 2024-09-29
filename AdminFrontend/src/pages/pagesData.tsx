import { routerType } from "../types/router.types";
import ColorSystem from "./ColorSystem/ColorSystem";
import AddItem from "./AddItem/AddItem";
import InStock from "./InStock/InStock";

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
    {
        path: "InStock",
        element: <InStock />,
        title: "In Stock"
    },
];

export default pagesData;