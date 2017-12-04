[%bs.raw {|require('./app.css')|}];

[@bs.module] external logo : string = "./logo.svg";

/* open Regql; */
let query = {|
  query company($id: ID) {
    company: Company (id: $id) {
      id
      name
    }
  }
|};

type company = {
  id: string,
  name: string
};

type data = {company};

let company = (json) =>
  Json.Decode.{id: json |> field("id", string), name: json |> field("name", string)};

let data = (json) => Json.Decode.{company: json |> field("company", company)};

module Container = {
  type shape = data;
  type variables = {. id: string};
  let decoder = data;
};

module FetchCompany = Gql.Client(Container);

let component = ReasonReact.statelessComponent("App");

let make = (~message, _children) => {
  ...component,
  render: (_self) => {
    let variables = Some({"id": "cj9pnqcwi00k801059imcqojg"});
    <div className="App">
      <div className="App-header">
        <img src=logo className="App-logo" alt="logo" />
        <h2> (ReasonReact.stringToElement(message)) </h2>
      </div>
      <p className="App-intro">
        (ReasonReact.stringToElement("To get started, edit"))
        <code> (ReasonReact.stringToElement(" src/app.re ")) </code>
        (ReasonReact.stringToElement("and save to reload."))
      </p>
      <FetchCompany query variables>
        (
          (response) =>
            switch response {
            | Loading => <div> (ReasonReact.stringToElement("Loading")) </div>
            | Failed(error) => <div> (ReasonReact.stringToElement(error)) </div>
            | Loaded(result) => <div> (ReasonReact.stringToElement(result.company.name)) </div>
            }
        )
      </FetchCompany>
    </div>
  }
};