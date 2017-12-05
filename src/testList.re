/* [%bs.raw {|require('./app.css')|}];

[@bs.module] external logo : string = "./logo.svg";

/* open Regql; */
let query = {|
  query allCompanies($filter: String) {
    allCompanies(filter: 
    { name_contains: $filter
    }) {
      id
      name
    }
  }
|};

type company = {
  id: string,
  name: string
};


type data = {allCompanies: list(company)};

/* type companies = {allCompanies: array(company)}; */

/* type company = {
  id: string,
  name: string
}; */

let company = (json) =>
  Json.Decode.{id: json |> field("id", string), name: json |> field("name", string)};
  
/* let data = (json) => Json.Decode.{company: json |> field("company", company)}; */

let data = (json) => Json.Decode.field("allCompanies", Json.Decode.array(company), json);

module Container = {
  type shape = data;
  type variables = {. filter: string};
  let decoder = data;
};

module FetchCompanies = Gql.Client(Container);

let component = ReasonReact.statelessComponent("App");

let make = (~message, _children) => {
  ...component,
  render: (_self) => {
    let variables = Some({"filter": "tas"});
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
      <FetchCompanies query variables>
        (
          (response) =>
            switch response {
            | Loading => <div> (ReasonReact.stringToElement("Loading")) </div>
            | Failed(error) => <div> (ReasonReact.stringToElement(error)) </div>
            | Loaded(result) => 
            <div> 
            (
              ReasonReact.arrayToElement(
                Array.map(
                  (company) => <div> (ReasonReact.stringToElement(result.company.name)) </div>
                )
              )
            )
            </div>
          }
        )
      </FetchCompanies>
    </div>
  }
}; */