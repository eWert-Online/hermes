module Client = Warp_Client;
module Header = Warp_Header;
module Method = Warp_Method;
module QueryString = Warp_QueryString;
module FormData = Warp_FormData;
module ResponseType = Warp_ResponseType;
module Types = Warp_Types;
module Event = Warp_Event;
module Settings = Warp_Settings;

let send:
  type a.
    Types.Client.t(Types.ResponseType.payload(a)) => option(unit => unit) =
  client => {
    let xhr = Warp_XHR.make();

    let queryString =
      Belt.List.map(client.queryString, ((key, value)) => {
        key ++ "=" ++ value
      })
      ->Belt.List.toArray
      ->Js.Array.joinWith("&", _);

    let formData =
      Belt.List.map(client.formData, ((key, value)) => {key ++ "=" ++ value})
      ->Belt.List.toArray
      ->Js.Array.joinWith("&", _);

    let url =
      if (Js.List.isEmpty(client.queryString)) {
        client.url;
      } else {
        client.url ++ "?" ++ queryString;
      };

    xhr->Warp_XHR.open_(
      ~url,
      ~method=
        switch (client.method) {
        | OPTIONS => "OPTIONS"
        | GET => "GET"
        | HEAD => "HEAD"
        | POST => "POST"
        | PUT => "PUT"
        | DELETE => "DELETE"
        | TRACE => "TRACE"
        | CONNECT => "CONNECT"
        },
      ~async=client.async,
      (),
    );

    let _ = xhr->Warp_XHR.setWithCredentials(client.withCredentials);
    let _ = xhr->Warp_XHR.setTimeout(client.timeout);

    Belt.List.forEach(client.headers, ((key, value)) => {
      xhr->Warp_XHR.setRequestHeader(key, value)
    });

    switch (client.onProgess) {
    | Some(onProgress) => xhr->Warp_XHR.onProgress(evt => onProgress(evt))
    | None => ()
    };

    switch (client.onAbort) {
    | Some(onAbort) => xhr->Warp_XHR.onAbort(_ => onAbort())
    | None => ()
    };

    switch (client.onLoad) {
    | Some(onLoad) =>
      xhr->Warp_XHR.onError(_ => {
        onLoad(Types.ResponseType.Error(xhr->Warp_XHR.statusText))
      });
      xhr->Warp_XHR.onTimeout(_ => {
        onLoad(Types.ResponseType.Error(xhr->Warp_XHR.statusText))
      });

      xhr->Warp_XHR.onLoad(_ => {
        switch (client.responseType) {
        | Types.ResponseType.TextResponse(_) =>
          onLoad(
            Types.ResponseType.Ok(
              Types.ResponseType.TextResponse(
                xhr->Warp_XHR.responseText->Js.Nullable.toOption,
              ),
            ),
          )
        | Types.ResponseType.JSONResponse(_) =>
          onLoad(
            Types.ResponseType.Ok(
              Types.ResponseType.JSONResponse(
                xhr->Warp_XHR.responseJson->Js.Nullable.toOption,
              ),
            ),
          )
        | Types.ResponseType.DocumentResponse(_) =>
          onLoad(
            Types.ResponseType.Ok(
              Types.ResponseType.DocumentResponse(
                xhr->Warp_XHR.responseDocument->Js.Nullable.toOption,
              ),
            ),
          )
        | Types.ResponseType.ArrayBufferResponse(_) =>
          onLoad(
            Types.ResponseType.Ok(
              Types.ResponseType.ArrayBufferResponse(
                xhr->Warp_XHR.responseArrayBuffer->Js.Nullable.toOption,
              ),
            ),
          )
        }
      });
    | None => ()
    };

    switch (client.method, Js.List.isEmpty(client.formData)) {
    | (GET, _)
    | (HEAD, _)
    | (_, true) => xhr->Warp_XHR.send
    | (_, false) =>
      xhr->Warp_XHR.setRequestHeader(
        "Content-type",
        "application/x-www-form-urlencoded",
      );
      xhr->Warp_XHR.sendString(formData);
    };

    Some(() => {Warp_XHR.abort(xhr)});
  };