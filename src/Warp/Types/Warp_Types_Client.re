type t('a) = {
  url: string,
  method: Warp_Types_Method.t,
  timeout: int,
  async: bool,
  withCredentials: bool,
  queryString: list((string, string)),
  formData: list((string, string)),
  headers: list((string, string)),
  responseType: 'a,
  onLoad: option(Warp_Types_ResponseType.t('a) => unit),
  onProgess: option(Dom.progressEvent => unit),
  onAbort: option(unit => unit),
};