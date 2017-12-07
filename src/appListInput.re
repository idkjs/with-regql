[%bs.raw {|require('./app.css')|}];

[@bs.module] external logo : string = "./logo.svg";

type state = {
  filter:string
};

type action =
  | InputChange(string);

let query = {|
  query allCompanies($filter: String) {
    companies: allCompanies(filter: 
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

type companies = {
  companies:array(company)
};

let company = (json) =>
  Json.Decode.{
    id: json |> field("id", string),
    name: json |> field("name", string)
  };

let companies = (json) =>
  Json.Decode.{
    companies: json |> field("companies", array(company))
  };
/* let data = (json) => Json.Decode.field("allCompanies", Json.Decode.array(company), json); */

module Container = {
  type shape = companies;
  type variables = {filter:string};
  /* type variables; */
  let decoder = companies;
};

module FetchCompanies = Gql.Client(Container);

/* function to get text value from our input box */
let valueFromEvent = (evt) : string => (
  evt
  |> ReactEventRe.Form.target
  |> ReactDOMRe.domElementToObj
)##value;

/* standalone module for our input element */
module Input = {
  type state = string;
  let component = ReasonReact.reducerComponent("Input");
  let make = (~onSubmit, _) => {
    ...component,
    initialState: () => "",
    reducer: (newText, _text) => ReasonReact.Update(newText),
    render: ({state: text, reduce}) =>
      <input
        value=text
        _type="text"
        placeholder="Search for Company Names"
        onChange=(reduce((evt) => valueFromEvent(evt)))
        onKeyDown=((evt) => 
          if (ReactEventRe.Keyboard.key(evt) == "Enter") {
            onSubmit(text);
            (reduce(() => ""))()
          }
        )
      />
  };
};

let component = ReasonReact.reducerComponent("AppListInput");
let make = (~message, _children) => {
  ...component,
  initialState: () => {filter:""},
  reducer: (action, state) => 
    switch action {
    | InputChange(text) => ReasonReact.Update({filter:text})
    },
  render: ({state, reduce}) => {
    let variables = Some({
      "filter" : state.filter
    });
    Js.log(variables);
    Js.log(query);
    Js.log(state);
    <div className="App">
      <div className="App-header">
        <img src=logo className="App-logo" alt="logo" />
        <h2> (ReasonReact.stringToElement(message)) </h2>
      </div>
      <Input onSubmit=(reduce((text) => InputChange(text))) />
      <div>
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
                    Array.map((company) => <div>(ReasonReact.stringToElement(company.name)) </div>,
                    result.companies)
                  )
                )
                </div> 
              }
          )
        </FetchCompanies>
      </div>
    </div>
  }
};