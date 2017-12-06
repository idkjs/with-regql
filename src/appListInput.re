[%bs.raw {|require('./app.css')|}];

[@bs.module] external logo : string = "./logo.svg";


/* aren't strings, nor references, nor some special data type. 
They're called "constructors" (or "tag"). The | bar 
separates each constructor. */
/* type state = Init(string) | HasQuery(string); */
type state = {
  inputValue: string,
  variables: string
};

type action =
  | InputChange(string)
  | EnterPressed;

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

/* type data = {company:company}; */

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
  let decoder = companies;
};

module FetchCompanies = Gql.Client(Container);

let component = ReasonReact.reducerComponent("AppListInput");

let make = (~message, _children) => {
  ...component,
  /* initialState: () =>  Init(""), */
  initialState: () => {inputValue:"", variables:""},
  reducer: (action, state) => {
    switch action {
      | InputChange(newVal) => ReasonReact.Update({...state, inputValue:newVal })
      | EnterPressed => 
      ReasonReact.UpdateWithSideEffects(
        {...state, inputValue: "", variables: state.inputValue},
          ((self) => Js.log("Updated State: " ++ self.state.variables))
        )
    }
  },
  render: (self) => {
    let handleInputChange = (e) => {
      let value = ReactDOMRe.domElementToObj(ReactEventRe.Form.target(e))##value;
      InputChange(value)
    };
    let handleKeyDown = (e) => {
      let key = ReactEventRe.Keyboard.key(e);
      if (key == "Enter") {
        key |> Js.log;
        self.reduce(() => EnterPressed)()
        } else {
        ()
      }
    };
    let variables = Some({
      "filter": self.state.variables
    });
    /* let variables = Some({"filter": "ta"}); */
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
      <div>
        <input
          _type="text"
          value=self.state.inputValue
          onChange=(self.reduce(handleInputChange))
          onKeyDown=handleKeyDown
        />
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