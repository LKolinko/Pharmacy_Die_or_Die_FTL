import { routerType } from "../types/router.types";
import ColorSystem from "./ColorSystem/ColorSystem";

const pagesData: routerType[] = [
    {
        path: "ColorSystem",
        element: <ColorSystem />,
        title: "Color System"
    },
];

export default pagesData;