[%bs.raw {|require('./index.css')|}];

[@bs.module "./registerServiceWorker"] external register_service_worker : unit => unit = "default";

/* ReactDOMRe.renderToElementWithId(<AppList message="Welcome to React and Reason" />, "root"); */
ReactDOMRe.renderToElementWithId(<App message="Welcome to React and Reason" />, "root");

register_service_worker();
