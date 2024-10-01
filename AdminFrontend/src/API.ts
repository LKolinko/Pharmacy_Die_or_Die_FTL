const requestAdd = async (num: string, Fbase: string, Sbase: string): Promise<any> => {
    try {
        const response = await fetch('http://0.0.0.0:1234/api', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ num, Fbase, Sbase }) || null
        });
        return await response.json();
    } catch (e) {
        console.log(e);
    }
}
export {requestAdd};