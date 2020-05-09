open Warp_Types_Client;

let make = (~url, ~method) => {
  {
    url,
    method,
    timeout: 0,
    async: true,
    withCredentials: false,
    username: None,
    password: None,
    overrideMimeType: None,
    queryString: [],
    responseType: Warp_Types_ResponseType.TextResponse(None),
    formData: [],
    headers: [],
    onLoad: None,
    onLoadWithStatusCode: None,
    onProgess: None,
    onAbort: None,
  };
};